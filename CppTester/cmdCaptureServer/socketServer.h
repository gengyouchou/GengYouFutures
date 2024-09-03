
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <array>

#define PORT 30666

extern char buffer[10240];
void thread_socket();