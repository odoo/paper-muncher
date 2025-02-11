# Protocol

This document describes the "HTTP over pipe" mode of PaperMuncher and the corresponding wire protocol interactions. It outlines how to start PaperMuncher in "HTTP over pipe" mode, as well as the format and flow of requests, responses, and result submissions.

---

## 1. Overview

"HTTP over pipe" mode allows PaperMuncher to fetch content, process it, and submit the processed result back via HTTP. The primary use case is converting web pages or other retrievable resources into PDF documents, though the mechanism can be extended for other transformations.

---

## 2. Starting PaperMuncher in "HTTP over pipe" Mode

To start PaperMuncher in "HTTP over pipe" mode, use the following command:

```bash
paper-muncher print <input> -o <output> --httpipe [options]
```

where:

- `<input>`
  The resource to convert. This can be a file path, or a URL if you are using "HTTP over pipe" mode.

- `-o <output>`  
  Specifies the output file path.

- `--httpipe`
  Activates "HTTP over pipe" mode, indicating the input should be fetched via HTTP, and the result optionally submitted via an HTTP POST.

- `[options]`
  Additional flags or configuration parameters relevant to your setup (e.g., SSL options, custom headers, etc.).

---

## 3. Wire Protocol

This section covers the HTTP messages exchanged when PaperMuncher operates in "HTTP over pipe" mode.

### 3.1 HttpPipe Request

PaperMuncher issues an HTTP GET request to retrieve the resource specified by `<input>`:

```
GET <url> HTTPIPE/1
User-Agent: PaperMuncher/0.1.0 Vaev/0.1.0
```

- **Method**: `GET`  
- **URL**: `<url>` (Provided as `<input>` from the command line)  
- **HTTP Version**: `HTTPIPE/1`  
- **User-Agent**: `PaperMuncher/0.1.0 Vaev/0.1.0`  

### 3.2 HttpPipe Response

The server responds to the GET request, typically returning an HTML page or other resource content. A valid "HTTP over pipe" response might look like:

```
HTTPIPE/1 200
content-Type: text/html; charset=UTF-8
content-Length: 14513
```

- **HTTP Status**: `200 OK`
  Indicates a successful retrieval of the resource.

- **content-Type**: `text/html; charset=UTF-8` (or other valid MIME type)
  Specifies the resource type. PaperMuncher will parse and process this content accordingly.

- **content-Length**: `14513`
  Size of the returned resource body (in bytes).

---

### 3.3 HttpPipe Result

After processing the retrieved resource (e.g., converting it to a PDF), PaperMuncher will post the result back to a specified URL via an HTTP POST:

```
POST <url> HTTPIPE/1
User-Agent: PaperMuncher/0.1.0 Vaev/0.1.0
Content-Type: application/pdf
Content-Length: 131345
```

- **Method**: `POST`  
- **URL**: `<url>` (derived from the `-o` arguments provided to PaperMuncher.)
- **HTTP Version**: `HTTPIPE/1`  
- **User-Agent**: `PaperMuncher/0.1.0 Vaev/0.1.0`  
- **Content-Type**: `application/pdf`  
  Indicates the resulting file is in PDF format.

- **Content-Length**: `131345`  
  Size of the PDF data (in bytes).

---

## 4. Example Flow

1. **Client Invocation**  
   A user runs PaperMuncher to fetch `<url>` and produce `output.pdf`:
   ```bash
   paper-muncher print https://example.com -o output.pdf --httpipe
   ```

2. **Initial GET**  
   PaperMuncher sends a GET request over HTTPIPE/1:
   ```
   GET https://example.com HTTPIPE/1
   User-Agent: PaperMuncher/0.1.0 Vaev/0.1.0
   ```
   The server returns an HTML document.

3. **Processing**
   PaperMuncher converts the HTML into a PDF.

4. **Result POST (Optional)**
   PaperMuncher will then POST the PDF file back:

   ```
   POST https://example.com/upload HTTPIPE/1
   User-Agent: PaperMuncher/0.1.0 Vaev/0.1.0
   Content-Type: application/pdf
   Content-Length: 131345

   [binary PDF data]
   ```

5. **Completion**
   PaperMuncher exits
