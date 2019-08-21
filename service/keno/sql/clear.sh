HOST=$1
USER=$2
PASS=$3

mysql -h $HOST -u $USER -p$PASS < create_keno.sql
mysql -h $HOST -u $USER -p$PASS < create_seed.sql
mysql -h $HOST -u $USER -p$PASS < create_test.sql
