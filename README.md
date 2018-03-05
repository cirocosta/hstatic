<h1 align="center">hstatic ⚡️ </h1>

<h5 align="center">An incomplete and small example http server in C</h5>

<br/>

[![Build Status](https://travis-ci.org/cirocosta/hstatic.svg?branch=master)](https://travis-ci.org/cirocosta/hstatic)


### HTTP Request

Whenever a client connection is accepted, `hstatic` starts the process of retrieving the request from the client and parsing that request such that it can interpret it and respond accordingly.

It does so by reading chunks of data and attempting to parse that message.

```

        read(client_fd, buf, n)

                -->     buf gets filled with initial bytes from the
                        message

                -->     checks if that's a complete HTTP request by
                        looking at the `start-line` and `*(message-header CRLF)`.
                        
                                -       any message with a `body` is considered an
                                        error

```


### TODO

- [ ] read timeout
- [ ] write timeout
- [ ] limit headers

