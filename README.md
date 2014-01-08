#WebServer

Web Server written in pure C in Linux, that can be configurated from file, response with static files on GET requests

###Requirements:
#####1. OS - Linux

#####2. Support multiple simultaneous connections

#####3. Specific HTTP features support:
  * Support keep-alive http attribute
  * Chunked transfer encoding

  ```
  Chunked transfer encoding allows a server to maintain an HTTP persistent 
  connection for dynamically generated content.
  But in item 8. described that "It is enough to support only static content".
  So do we need this option?
  ```

#####4. Access logging using W3C log format.
  The common logfile format is as follows:

  `remotehost [date] "request" status bytes`

  Where:

  ```
  remotehost - Remote hostname (or IP number if DNS hostname is not available, or if DNSLookup is Off.)
  [date] - Date and time of the request.
  "request" - The request line exactly as it came from the client.
  status - The HTTP status code returned to the client.
  bytes - The content-length of the document transferred.
  ```

  Example of one record:

  `127.0.0.1 [22/Nov/2013:16:59:56] "GET /index.htm HTTP/1.1" 200 703`

#####5. Debug logging (any format)
  Errors will be shown using next format:
  
  `[date] Error_Text`

#####6. Configuration file (any format)

  ```
  # Comment example and data example will be used in next format:
  #key=value
  logAccess=log/access.log
  logErrors=log/errors.log
  port to listen (default 80)
  portListen=80
  interfaces to listed on (default all)
  #Would be nice to make a command to show all available interfaces
  interfaceToListen=127.0.0.1
  default document (index.html)
  defaultPage=index.htm
  specified root folder
  rootFolder=www/
  ```

#####7. Performance, Stability and Security:
  * The server should use all available system resources to handle as much requests as possible as quickly as possible.
  * In case of inability to handle some request the server should return an error to the client(if possible) and write error log.
  * the server should be coded in a manner that prevents attacks by manipulating input requests.

#####8. Limitations:
  * It is enough to support only static content
  * It is enough to implement GET command, other commands are not required

#####9. Programming language end environment: 
  * C language
  * Compile using make from command line

#####10. Future plans (informational)
  * Make posibility to handle virtual hosts (many root directories)
  * Parse multiple values for one key in parameters (key=value1 value2 value3)
  * Implement other types of requests (HEAD, POST)
  * Implement using PHP, Python, or even own interpretated or compiled language
  * Add posibility to send custom error pages if such are defined in configuration
