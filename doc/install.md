# For end-users

1. **Download** the prebuilt package for your distribution from the [nightly releases](https://github.com/odoo/paper-muncher/releases/tag/nightly).
2. **Install** the package using your system's package manager.
3. **Add** `/opt/paper-muncher/bin` to your `PATH` environment variable.
4. **Verify** the installation by running `paper-muncher --help` to see available options.
5. **Learn more** by visiting the [official documentation](https://odoo.github.io/paper-muncher/usage.html).

---

# For development


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
