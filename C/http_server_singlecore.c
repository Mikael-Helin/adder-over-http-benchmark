#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

void request_handler(struct evhttp_request *req, void *arg) {
    struct evbuffer *buf = evbuffer_new();
    if (!buf) {
        puts("failed to create response buffer");
        return;
    }

    const char *uri = evhttp_request_get_uri(req);
    char result[64];
    int num1, num2;
    
    // Parse the numbers from the URI
    sscanf(uri, "/%d/%d", &num1, &num2);
    snprintf(result, sizeof(result), "{\"result\": %d}", num1 + num2);

    evbuffer_add_printf(buf, "%s", result);

    evhttp_send_reply(req, 200, "OK", buf);

    evbuffer_free(buf);
}

int main(int argc, char **argv) {
    struct event_base *base;
    struct evhttp *http;
    struct evhttp_bound_socket *handle;
    
    base = event_base_new();
    if (!base) {
        puts("Couldn't create an event_base");
        return 1;
    }

    http = evhttp_new(base);
    if (!http) {
        puts("Couldn't create evhttp");
        return 1;
    }

    evhttp_set_gencb(http, request_handler, NULL);

    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", 4000);
    if (!handle) {
        puts("Couldn't bind to port 4000");
        return 1;
    }

    printf("Server started on port 4000\n");

    event_base_dispatch(base);

    evhttp_free(http);
    event_base_free(base);

    return 0;
}