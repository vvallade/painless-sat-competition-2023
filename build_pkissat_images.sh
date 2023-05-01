# /bin/sh
cd common
docker build -t satcomp-pkissat:common .
cd ..

cd leader
docker build -t satcomp-pkissat:leader .
cd ..
