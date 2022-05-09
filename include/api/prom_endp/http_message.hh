#pragma once

#define HTTP_MESSAGE_H_
#include <algorithm>
#include <cctype>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <type_traits>
#include <map>
#include <string>
#include <utility>
#include "common.hh"
#include "api/prom_endp/uri.hh"

namespace dibibase::api::prom_endp {


enum class DIBIBASE_PUBLIC HttpMethod {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  PATCH
};
//  HTTP_1.1 is supported ((only))
enum class DIBIBASE_PUBLIC HttpVersion {
  HTTP_0_9 = 9,
  HTTP_1_0 = 10,
  HTTP_1_1 = 11,
  HTTP_2_0 = 20
};
enum class DIBIBASE_PUBLIC HttpStatusCode {
  Continue = 100,
  SwitchingProtocols = 101,
  EarlyHints = 103,
  Ok = 200,
  Created = 201,
  Accepted = 202,
  NonAuthoritativeInformation = 203,
  NoContent = 204,
  ResetContent = 205,
  PartialContent = 206,
  MultipleChoices = 300,
  MovedPermanently = 301,
  Found = 302,
  NotModified = 304,
  BadRequest = 400,
  Unauthorized = 401,
  Forbidden = 403,
  NotFound = 404,
  MethodNotAllowed = 405,
  RequestTimeout = 408,
  ImATeapot = 418,
  InternalServerError = 500,
  NotImplemented = 501,
  BadGateway = 502,
  ServiceUnvailable = 503,
  GatewayTimeout = 504,
  HttpVersionNotSupported = 505
};

// converting between string or integer to enum classes
std::string to_string(HttpMethod method);
std::string to_string(HttpVersion version);
std::string to_string(HttpStatusCode status_code);
HttpMethod string_to_method(const std::string& method_string);
HttpVersion string_to_version(const std::string& version_string);

/*
*  HttpMessageInterface class is used to handle HTTP headers 
*/
class DIBIBASE_PUBLIC HttpMessageInterface {
public:
  HttpMessageInterface() : version_(HttpVersion::HTTP_1_1) {}
  virtual ~HttpMessageInterface() = default;

  void SetHeader(const std::string& key, const std::string& value) { headers_[key] = std::move(value); }
  void RemoveHeader(const std::string& key) { headers_.erase(key); }
  void ClearHeader() { headers_.clear(); }
  void SetContent(const std::string& content) {
    content_ = std::move(content);
    SetContentLength();
  }
  void ClearContent(const std::string& content) {
    content_.clear();
    SetContentLength();
  }

  HttpVersion version () const { return version_; }
  std::string header(const std::string& key) const {
    if (headers_.count(key) > 0)
      return headers_.at(key);
    return std::string();
  }
  std::map<std::string, std::string> headers() const { return headers_; }
  std::string content() const { return content_; }
  size_t content_length() const { return content_.length(); }

protected:
  HttpVersion version_;
  std::map<std::string, std::string> headers_;
  std::string content_;

  void SetContentLength() { SetHeader("Content-Length", std::to_string(content_.length())); }
};

/*
 HttpRequest object represents one HTTP request
 It has (HTTP_method ,URI)
 the corresponding resource and action 
*/
class DIBIBASE_PUBLIC HttpRequest : public HttpMessageInterface {
public:
  HttpRequest() : method_(HttpMethod::GET) {}
  ~HttpRequest() = default;

  void SetMethod(HttpMethod method) { method_ = method; }
  void SetUri(const Uri& uri) { uri_ = std::move(uri); }

  HttpMethod method() const { return method_; }
  Uri uri() const { return uri_; }

  friend std::string to_string(const HttpRequest& request);
  friend HttpRequest string_to_request(const std::string& request_string);

private:
  HttpMethod method_;
  Uri uri_;
};

//HTTPResponse object is the same as HttpRequest object including (HTTP headers,content)

class DIBIBASE_PUBLIC HttpResponse : public HttpMessageInterface {
public:
  HttpResponse() : status_code_(HttpStatusCode::Ok) {}
  HttpResponse(HttpStatusCode status_code) : status_code_(status_code) {}
  ~HttpResponse() = default;

  void SetStatusCode(HttpStatusCode status_code) { status_code_ = status_code; }

  HttpStatusCode status_code() const { return status_code_; }

  friend std::string to_string(const HttpResponse& request, bool send_content );
  friend HttpResponse string_to_response(const std::string& response_string);

private:
  HttpStatusCode status_code_;
};

// Utility functions to convert HTTP message objects to string and vice versa
std::string to_string(const HttpRequest& request);
std::string to_string(const HttpResponse& response, bool send_content);
HttpRequest string_to_request(const std::string& request_string);
HttpResponse string_to_response(const std::string& response_string);

} // namespace dibibase::api::prom_endp