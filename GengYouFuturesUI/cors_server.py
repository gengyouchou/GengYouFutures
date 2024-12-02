from http.server import SimpleHTTPRequestHandler, HTTPServer

class CORSRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        # 添加 CORS 头部
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        super().end_headers()

def run(server_class=HTTPServer, handler_class=CORSRequestHandler):
    server_address = ('', 8000)
    httpd = server_class(server_address, handler_class)
    print("Serving on port 8000 with CORS enabled")
    httpd.serve_forever()

if __name__ == '__main__':
    run()
