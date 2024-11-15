#include <cstdarg>
#include <iostream>
#include <libwebsockets.h> // Include libwebsockets library
#include <pthread.h>
#include <string>

#define MAX_BUFFER_SIZE 1024
#define WEBSOCKET_PORT 8080

// WebSocket context and clients
static struct lws_context *context;
static struct lws *client_wsi;

// Mutex for thread safety
static pthread_mutex_t ws_mutex = PTHREAD_MUTEX_INITIALIZER;

// WebSocket protocol callback
static int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_ESTABLISHED:
        std::cout << "Client connected." << std::endl;
        client_wsi = wsi; // Save client connection
        break;

    case LWS_CALLBACK_CLOSED:
        std::cout << "Client disconnected." << std::endl;
        client_wsi = NULL; // Clear client connection
        break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
        // No specific action here; sending data occurs on demand
        break;

    default:
        break;
    }
    return 0;
}

// WebSocket protocols
static const struct lws_protocols protocols[] = {
    {"ws-protocol", callback_ws, 0, MAX_BUFFER_SIZE},
    {NULL, NULL, 0, 0}};

// Initialize WebSocket server
void init_websocket_server()
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = WEBSOCKET_PORT;
    info.protocols = protocols;

    context = lws_create_context(&info);
    if (!context)
    {
        std::cerr << "Failed to create WebSocket context." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "WebSocket server started at ws://localhost:" << WEBSOCKET_PORT << std::endl;
}

// Send data to WebSocket client
void send_to_websocket(const char *message)
{
    if (!client_wsi)
    {
        std::cout << "No active WebSocket client." << std::endl;
        return;
    }

    // Lock to ensure thread safety
    pthread_mutex_lock(&ws_mutex);

    // Allocate buffer for WebSocket framing
    unsigned char buffer[LWS_PRE + MAX_BUFFER_SIZE];
    size_t len = strlen(message);

    if (len > MAX_BUFFER_SIZE)
    {
        std::cout << "Message too long to send." << std::endl;
        pthread_mutex_unlock(&ws_mutex);
        return;
    }

    // Copy message to WebSocket buffer
    memcpy(&buffer[LWS_PRE], message, len);

    // Send message
    lws_write(client_wsi, &buffer[LWS_PRE], len, LWS_WRITE_TEXT);

    // Unlock
    pthread_mutex_unlock(&ws_mutex);
}

// Custom printf function
void ws_printf(const char *format, ...)
{
    char buffer[MAX_BUFFER_SIZE];
    va_list args;

    // Format the string
    va_start(args, format);
    vsnprintf(buffer, MAX_BUFFER_SIZE, format, args);
    va_end(args);

    // Send formatted string to WebSocket
    send_to_websocket(buffer);

    // Optionally print to console (for debugging)
    std::cout << buffer;
}

// WebSocket server loop
void *websocket_server_loop(void *arg)
{
    while (1)
    {
        lws_service(context, 1000); // Process WebSocket events
    }
    return NULL;
}

int main()
{
    // Initialize WebSocket server
    init_websocket_server();

    // Start WebSocket server loop in a separate thread
    pthread_t ws_thread;
    pthread_create(&ws_thread, NULL, websocket_server_loop, NULL);

    // Example usage of ws_printf
    for (int i = 0; i < 10; i++)
    {
        ws_printf("Log message %d: Hello WebSocket!\n", i);
        sleep(1); // Simulate data updates
    }

    // Cleanup
    lws_context_destroy(context);
    pthread_mutex_destroy(&ws_mutex);
    return 0;
}
