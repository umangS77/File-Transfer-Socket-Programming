# File Transfer in Socket Programming  

### Overview  
The server and client will be two different programs that will be run in different directories. The client will create a connection to the server and send requests to download files to the client directory. It will write the files into its own directory. Progress is printed and errors for cases such as requesting missing files are handled appropriately.  

### Syntax
```sh
client> ​get <file1> <file2> <file3> // Files requested are downloaded in the client directory.   
client> ​exit // Connection with the server is closed.  
```