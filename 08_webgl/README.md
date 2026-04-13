### Why you need a web server
Modern browsers block `file://` access to JavaScript ES modules, WebGL assets, and texture loading for security reasons. This means that opening `index.html` directly in the browser will not work properly. You need to serve the project through a local web server so that these resources load correctly.

---

### Option 1: Use the Node-based HTTP server

Install the HTTP server:

```bash
npm install
```

Run the HTTP server:

```bash
npm start
```

---

### Option 2: Use Python's built-in HTTP server
If you have Python installed, you can use it to run a simple server:

- **Python 3:**

```bash
python -m http.server 8000
```

Then open your browser to: [http://localhost:8000](http://localhost:8000)
