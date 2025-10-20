# Install

Installing paper-muncher is currently only supported on Linux and macOS. We plan to add support for Windows in the future.

```sh
# Clone the repository
git clone https://github.com/odoo/paper-muncher

# Build and install the project
cd paper-muncher
./ck package install --release --prefix=$HOME/.local

# Add the binary to your PATH, add this to your .bashrc to make it permanent
export PATH=$PATH:$HOME/.local/bin

# Render a webpage to PDF
paper-muncher index.html -o output.pdf

# For more options, run
paper-muncher --help
```
