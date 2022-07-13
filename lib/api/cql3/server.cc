#include "api/cql3/server.hh"
#include "db/database.hh"
#include "api/cql3/server.hh"
#include "api/prom_endp/uri.hh"
#include "api/prom_endp/http_message.hh"
#include "api/prom_endp/http_server.hh"


using namespace dibibase::api::cql3;
using namespace dibibase::api::prom_endp;


Server::Server(const int port) {
  std::shared_ptr<db::Database> db = std::make_shared<db::Database>("database");
  std::string test="INIT";  
  std::string host = "0.0.0.0";
  HttpServer prom_server(host, 8080);

  std::string ex_metric = "GOTO /metrics";
  auto Greetings = [ex_metric](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/plain");
    response.SetContent(ex_metric);
    return response;
  };
  prom_server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, Greetings);
  prom_server.RegisterHttpRequestHandler("/", HttpMethod::GET, Greetings);
  std::cout << "Starting the web prom_server.." << std::endl;
  prom_server.Start();
  std::cout << "prom_server listening on " << host << ":8080" << std::endl;

  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  int met[8] = {0,0,0,0,0,0,0,0};
  char buffer[MAX_MESSAGE_LEN];
  char new_buffer[MAX_MESSAGE_LEN];
  memset(buffer, 0, sizeof(buffer));

  sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_listen_fd < 0) {
    error("Error creating socket..\n");
  }

  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_listen_fd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0)
    error("Error binding socket (CQL Server).. \n");

  if (listen(sock_listen_fd, BACKLOG) < 0) {
    error("Error listening..\n");
  }
  printf("CQL server listening for connections on port: %d\n", port);

  struct epoll_event events[MAX_EVENTS];
  int new_events, sock_conn_fd;

  epollfd = epoll_create(MAX_EVENTS);
  if (epollfd < 0) {
    error("Error creating epoll (CQL Server).. \n");
  }
  ev.events = EPOLLIN;
  ev.data.fd = sock_listen_fd;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_listen_fd, &ev) == -1) {
    error("Error adding new listeding socket to epoll..\n");
  }
  printf("\n # Connection established (CQL Server) # \n");
  int count = 0;
  int flag =0;

  while (1) {
    /* 
    * epoll_wait waits fininte time (10 ms)
    */
    new_events = epoll_wait(epollfd, events, MAX_EVENTS, 10);


    // prometheus metrics at /metrics
    std::string met0_str = "# TYPE number_of_tables_created counter\nnumber_of_tables_created "+ std::to_string(met[0])+"\n";
    std::string met1_str = "# TYPE number_of_insertions counter\nnumber_of_insertions "+ std::to_string(met[1])+"\n";
    std::string met2_str = "# TYPE number_of_selections counter\nnumber_of_selections "+std::to_string(met[2])+"\n";
    std::string met3_str = "# TYPE time_of_insertion gauge\ntime_of_insertion "+std::to_string(met[3])+"\n";
    std::string met4_str = "# TYPE time_of_selection gauge\ntime_of_selection "+std::to_string(met[4])+"\n";
    std::string met5_str = "# TYPE number_of_deletetions counter\nnumber_of_deletions "+std::to_string(met[5])+"\n";
    std::string met6_str = "# TYPE time_of_deletion gauge\ntime_of_deletion "+std::to_string(met[6])+"\n";
    std::string met7_str = "# TYPE number_of_exceptions counter\nnumber_of_exceptions "+std::to_string(met[7])+"\n";
    std::string test = met0_str+met1_str+met2_str+met3_str+met4_str+met5_str+met6_str + met7_str;
    
    auto Greetings2 = [test](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/plain");
    response.SetContent(test);
    return response;
    };
    prom_server.RegisterHttpRequestHandler("/metrics",HttpMethod::HEAD, Greetings2);
    prom_server.RegisterHttpRequestHandler("/metrics", HttpMethod::GET, Greetings2);
    if(!prom_started) {
      printf("\n # Prometheus Endpoint is Up # \n");
      prom_started = true;
    }


    
    if (new_events == -1) {
      error("Error in epoll_wait (CQL Server)..\n");
    }

    for (int i = 0; i < new_events; ++i) {
      if (events[i].data.fd == sock_listen_fd) {
        sock_conn_fd = accept4(sock_listen_fd, (struct sockaddr *)&client_addr,
                               &client_len, SOCK_NONBLOCK);
        if (sock_conn_fd == -1) {
          error("Error accepting new connection (CQL Server)..\n");
        }

        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = sock_conn_fd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_conn_fd, &ev) == -1) {
          error("Error adding new event to epoll (CQL Server)..\n");
        }
      } else {
        int newsockfd = events[i].data.fd;
        int bytes_received = recv(newsockfd, buffer, MAX_MESSAGE_LEN, 0);
        if (bytes_received <= 0) {
          db->flush_metadata();
          continue;
        } else {
          std::vector<int> indeces;
          int no_of_queries = 0;
          for (int i=0 ; i< bytes_received; i++){
            if((buffer[i] == buffer[0] && buffer[i+1] == buffer[1]) ) {
              no_of_queries++; 
              indeces.push_back(i);
              }
          }
          indeces.push_back(bytes_received-1);


          for (int mq=0; mq < int(indeces.size())-1 ; mq++) {
          for(int j = indeces[mq] ;j<indeces[mq+1] ;j++){
            new_buffer[j-indeces[mq]] = buffer[j];
          }

          Frame f(new_buffer, indeces[mq+1]-indeces[mq]);
          f.parse();
          ServerMsg m(f); 
          int bytes_sent = m.CreateResponse(db,met);

          int index = 0;
          char div[1000];
          for (int i=0;i<bytes_sent;i++){
            if(m.Header[i]==m.Header[0] && m.Header[i+1]==m.Header[1] && m.Header[2]==255 ) index = i;
            if (index !=0) div[i-index] = m.Header[i];
          }
          if(index ==0)
          send(newsockfd, m.Header , bytes_sent, 0);
          else{
            send(newsockfd, m.Header , index+1, 0);
            for(int j=0;j<50;j++);
            send(newsockfd, div , bytes_sent-index, 0);
          }

          if ((strstr(query.c_str(),columns.c_str() ) ) && count == 0){
            count ++;
            flag = 1;
          }
          else if (flag==1) count ++;
          }

        }
      }
    }
  }
}
