# Install

Installing paper-muncher is currently only supported on Linux and macOS. We plan to add support for Windows in the future.

```sh
# Install the latest version of paper-muncher
git clone https://github.com/odoo/paper-muncher
cd paper-muncher
./ck package install --mixins=release --prefix=$HOME/.local/opt/paper-muncher

# Add the following to your shell profile:
export PATH=$HOME/.local/opt/paper-muncher/bin:$PATH

# To verify the installation, run:
paper-muncher --version
```
