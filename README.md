# HTTPd
Implementation of our own HTTP server in a group of 2 people, following the HTTP/1.1 specification defined by the RFCs.

To download our project :
```bash
wget https://github.com/TRKirua/HTTPd.git
-O "HTTPd.zip" &&
unzip ./"HTTPd.zip" &&
rm ./"HTTPd.zip"
```
To compile our project :
```bash
make
```

## Usage
To run our project :\
\
In one terminal launch the following command:
```bash
./httpd [--dry-run] [-a (start | stop | reload | restart)] server.conf
```

In another one launch your request with:
```bash
curl -i <ip> #GET
curl -i --head <ip> #HEAD
```

Or with:
```bash
echo -ne 'GET /<ip> HTTP/1.1\r\nHost: localhost\r\n\r\n' | nc localhost 4242 #GET
echo -ne 'HEAD /<ip> HTTP/1.1\r\nHost: localhost\r\n\r\n' | nc localhost 4242 #HEAD
```

## Clean project
To clean the project :
```bash
make clean
```

## Credits :
 * [Enes Ekici](https://github.com/TRKirua)
 * [Emon Barberis](https://github.com/EmonBar)
