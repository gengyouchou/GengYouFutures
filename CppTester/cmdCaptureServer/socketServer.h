
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <array>

#define PORT 30666

char buffer[10240]  = {0};
void thread_socket();