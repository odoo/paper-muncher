from cutekit import cli

@cli.command("tools/setup", "Setup the development environment")
def _():
    raise RuntimeError("Don't use ck directly, use ./ck instead.")
