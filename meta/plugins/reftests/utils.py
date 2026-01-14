from cutekit import model
from random import randint
from pathlib import Path


def fetchFile(manifests: model.Registry, component: str, path: str) -> str:
    """
    Fetches the text content of a file from a specific component's directory.

    Args:
        manifests: The component registry used to look up component information.
        component: The name of the component (e.g., "karm-core").
        path: The relative path to the file within that component's directory
              (e.g., "base/defs/error.inc").

    Returns:
        The entire content of the specified file as a string.

    Raises:
        AssertionError: If the specified component is not found in the registry.
    """
    component = manifests.lookup(component, model.Component)
    assert component is not None
    p = Path(component.dirname()) / path
    with p.open() as f:
        return f.read()


def fetchMessage(manifests: model.Registry, type: str) -> str:
    """
    Fetches a random message from a ".inc" file. (e.g., funny error/success messages)

    Args:
        manifests: The component registry used to look up component information.
        type: The type of message to fetch (e.g., "witty", "nice"), which
              corresponds to the name of the .inc file.

    Returns:
        A randomly selected message string from the fetched file.
    """

    messages = eval(
        "[" + fetchFile(manifests, "karm-core", "base/defs/" + type + ".inc") + "]"
    )
    return messages[randint(0, len(messages) - 1)]
