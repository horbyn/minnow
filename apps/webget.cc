#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

enum class HttpHeaderMethod
{
  GET = 0,
  POST
};
enum class HttpHeaderVersion
{
  HTTP_1_0 = 0,
  HTTP_1_1
};
enum class HttpHeaderAccept
{
  TEXT = 0,
  JSON
};
enum class HttpHeaderConnection
{
  CLOSE = 0,
  KEEP_ALIVE
};

struct HttpHeader
{
  HttpHeaderMethod method_;
  std::string path_;
  HttpHeaderVersion version_;
  std::string host_;
  HttpHeaderAccept accept_;
  HttpHeaderConnection connection_;

  HttpHeader( HttpHeaderMethod method,
              std::string path,
              HttpHeaderVersion version,
              std::string host,
              HttpHeaderAccept accept,
              HttpHeaderConnection connection )
    : method_( method )
    , path_( path )
    , version_( version )
    , host_( host )
    , accept_( accept )
    , connection_( connection )
  {}
  std::string get_header() const
  {
    std::string method = method_ == HttpHeaderMethod::GET ? "GET" : "POST";
    std::string version = version_ == HttpHeaderVersion::HTTP_1_0 ? "HTTP/1.0" : "HTTP/1.1";
    std::string accept = accept_ == HttpHeaderAccept::TEXT ? "text/html" : "application/json";
    std::string connection = connection_ == HttpHeaderConnection::CLOSE ? "close" : "keep-alive";
    return method + " " + path_ + " " + version + "\r\n" + "Host: " + host_ + "\r\n" + "Accept: " + accept + "\r\n"
           + "Connection: " + connection + "\r\n\r\n";
  }
};

void get_URL( const string& host, const string& path )
{
  /* In the get_URL function, implement the simple Web client as described in this file，
     using the format of an HTTP （Web） request that you used earlier.
     Use the TCPSocket and Address classes. */
  HttpHeader header { HttpHeaderMethod::GET,
                      path,
                      HttpHeaderVersion::HTTP_1_1,
                      host,
                      HttpHeaderAccept::TEXT,
                      HttpHeaderConnection::CLOSE };
  TCPSocket sock {};
  sock.set_blocking( true );
  sock.connect( Address { host, "80" } );
  auto write_bytes = sock.write( header.get_header() );
  if ( write_bytes != header.get_header().size() ) {
    throw runtime_error { "Error: bytes have been writen is not match with the header size" };
  }
  std::string recv_buff {};
  while ( sock.eof() != true ) {
    sock.read( recv_buff );
    std::cout << recv_buff;
  }
  sock.shutdown( SHUT_RDWR );
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
