#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8  // Adjust this based on your CPU core count

struct thread_arg {
    struct event_base *base;
    struct evhttp *http;
};

void request_handler(struct evhttp_request *req, void *arg) {
    struct evbuffer *buf = evbuffer_new();
    if (!buf) {
        puts("failed to create response buffer");
        return;
    }

    const char *uri = evhttp_request_get_uri(req);
    int num1, num2;
    
    // Parse the numbers from the URI
    sscanf(uri, "/%d/%d", &num1, &num2);

    evbuffer_add_printf(buf, "{\"result\": %d}", num1 + num2);
    
    struct evkeyvalq *headers = evhttp_request_get_output_headers(req);
    evhttp_add_header(headers, "Content-Type", "application/json");
    
    evhttp_send_reply(req, 200, "OK", buf);
    evbuffer_free(buf);
}

void *worker_thread(void *arg) {
    struct thread_arg *targ = (struct thread_arg *)arg;
    event_base_dispatch(targ->base);
    return NULL;
}

int main(int argc, char **argv) {
    if (evthread_use_pthreads() < 0) {
        puts("Failed to initialize libevent with pthread support");
        return 1;
    }

    struct event_base *bases[NUM_THREADS];
    struct evhttp *https[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    struct thread_arg thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        bases[i] = event_base_new();
        if (!bases[i]) {
            puts("Couldn't create an event_base");
            return 1;
        }

        https[i] = evhttp_new(bases[i]);
        if (!https[i]) {
            puts("Couldn't create evhttp");
            return 1;
        }

        evhttp_set_gencb(https[i], request_handler, NULL);
    }

    struct evhttp_bound_socket *handle = evhttp_bind_socket_with_handle(https[0], "0.0.0.0", 4000);
    if (!handle) {
        puts("Couldn't bind to port 4000");
        return 1;
    }

    for (int i = 1; i < NUM_THREADS; i++) {
        if (evhttp_accept_socket(https[i], evhttp_bound_socket_get_fd(handle)) != 0) {
            printf("Couldn't share listen socket with thread %d\n", i);
            return 1;
        }
    }

    printf("Server started on port 4000 with %d threads\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].base = bases[i];
        thread_args[i].http = https[i];
        if (pthread_create(&threads[i], NULL, worker_thread, &thread_args[i]) != 0) {
            printf("Failed to create thread %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        evhttp_free(https[i]);
        event_base_free(bases[i]);
    }

    return 0;
}