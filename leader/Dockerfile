################### Use pkissat
FROM satcomp-pkissat:common AS builder
USER root


################### Extract pkissat in run stage
FROM satcomp-infrastructure:leader AS pkissat_liaison
WORKDIR /
# Copy pkissat and solver scripts
COPY --from=builder /pkissat/pkissat pkissat

COPY --chown=ecs-user /init_solver.sh /competition
COPY --chown=ecs-user /run_solver.sh /competition
COPY --chown=ecs-user /solver /competition
USER ecs-user
RUN chmod +x /competition/init_solver.sh
RUN chmod +x /competition/run_solver.sh
RUN chmod +x /competition/solver