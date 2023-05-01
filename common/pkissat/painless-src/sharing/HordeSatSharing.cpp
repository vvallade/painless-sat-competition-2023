// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
//
// This file is part of PaInleSS.
//
// PaInleSS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

#include "../clauses/ClauseManager.h"
#include "../sharing/HordeSatSharing.h"
#include "../solvers/SolverFactory.h"
#include "../utils/Logger.h"
#include "../utils/Parameters.h"

HordeSatSharing::HordeSatSharing()
{
   this->literalPerRound = Parameters::getIntParam("shr-lit", 1500);
   this->initPhase = true;
   // number of round corresponding to 5% of the 5000s timeout
   this->roundBeforeIncrease = 250000000 / Parameters::getIntParam("shr-sleep", 500000);
   if (Parameters::getBoolParam("dup")) {
      filter = new BloomFilter();
   }
}

HordeSatSharing::~HordeSatSharing()
{
    for (auto pair : this->databases) {
        delete pair.second;
    }
}

void
HordeSatSharing::doSharing(int idSharer, const std::vector<SolverInterface *> & from,
                           const std::vector<SolverInterface *> & to)
{
   // static unsigned int round = 1;
   for (size_t i = 0; i < from.size(); i++) {
      int used, usedPercent, selectCount;
      int id = from[i]->id;

      if (!this->databases.count(id)) {
          this->databases[id] = new ClauseDatabase();
      }
      unfiltered.clear();
      filtered.clear();

      if (Parameters::getBoolParam("dup")) {
         from[i]->getLearnedClauses(unfiltered);
         for (ClauseExchange* c : unfiltered) {
            uint8_t count = filter->test_and_insert(c->checksum, 12);
            if (count == 1 || (count == 6 && c->lbd > 6) || (count == 11 && c->lbd > 2)) {
               ClauseManager::increaseClause(c);
               filtered.push_back(c);
            }
            if (count == 6 && c->lbd > 6) { // to promote (tiers2) and share
               c->lbd = 6;
               ++stats.promotionTiers2;
               ++stats.receivedDuplicas;
            } else if (count == 6) {
               ++stats.alreadyTiers2;
            } else if(count == 11 && c->lbd > 2) { // to promote (core) and share
               c->lbd = 2;
               ++stats.promotionCore;
               ++stats.receivedDuplicas;
            } else if (count == 11) {
               ++stats.alreadyCore;
            }
         }
         stats.receivedClauses += unfiltered.size();
         stats.receivedDuplicas += (unfiltered.size() - filtered.size());
      } else {
         from[i]->getLearnedClauses(filtered);
         stats.receivedClauses += filtered.size();
      }
      for (size_t k = 0; k < filtered.size(); k++) {
         this->databases[id]->addClause(filtered[k]);
      }
      unfiltered.clear();
      filtered.clear();

      used        = this->databases[id]->giveSelection(filtered, literalPerRound, &selectCount);
      usedPercent = (100 * used) / literalPerRound;

      stats.sharedClauses += filtered.size();

      if (usedPercent < 75) {
         from[i]->increaseClauseProduction();
         log(1, "Sharer %d production increase for solver %d.\n", idSharer,
             from[i]->id);
      } else if (usedPercent > 98) {
         from[i]->decreaseClauseProduction();
         log(1, "Sharer %d production decrease for solver %d.\n", idSharer,
             from[i]->id);
      }

      if (selectCount > 0) {
         log(1, "Sharer %d filled %d%% of its buffer %.2f\n", idSharer,
             usedPercent, used/(float)selectCount);
         this->initPhase = false;
      }
      // if (round >= this->roundBeforeIncrease) {
      //    this->initPhase = false;
      // }

      for (size_t j = 0; j < to.size(); j++) {
         if (from[i]->id != to[j]->id) {
            for (size_t k = 0; k < filtered.size(); k++) {
               ClauseManager::increaseClause(filtered[k], 1);
            }
            to[j]->addLearnedClauses(filtered);
         }
      }
      for (size_t k = 0; k < filtered.size(); k++) {
         ClauseManager::releaseClause(filtered[k]);
      }
   }
   // round++;
}

SharingStatistics
HordeSatSharing::getStatistics()
{
   return stats;
}
