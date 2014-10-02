find ./ -name "*.c"|xargs gcc -c -Wall -std=gnu99 -g -pg -D__DEBUG
gcc -Wall -std=gnu99 -g -pg -D__DEBUG__ httpd.o http_srv.o http_conn.o http_req.o http_res.o http_timer.o http_log.o http_conf.o http_fcache.o -o httpd_debug
sudo ./httpd_debug
