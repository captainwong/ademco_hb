import http.server
import sys


class Handler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        n = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(n).decode()
        with open(sys.argv[1], 'a', encoding='utf-8') as f:
            f.write(body + '\n')
        self.send_response(200)
        self.end_headers()

    def log_message(self, *a):
        pass


http.server.HTTPServer(('127.0.0.1', int(sys.argv[2])), Handler).serve_forever()
