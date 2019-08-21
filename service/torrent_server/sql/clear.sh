HOST=$1
USER=$2
PASS=$3

mysql -h $HOST -u $USER -p$PASS < create.sql
