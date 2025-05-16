from typing import Union


class Environ:
    """This class is a blueprint to handle request from paper-muncher
    """
    __slots__ = ('html',)

    html : Union[str, bytes, None]
    def __init__(self,  html: Union[str, bytes, None] = None):
        self.html = html

    def get_asset(self, path: str) -> bytes:
        """this method is used to get the assets

        Args:
            path (str): the URI to get the file
        """
        raise NotImplementedError("get_asset method should be implemented in subclasses")

    def set_html(self, html: Union[str, bytes]) -> None:
        """this method is used to set the html

        Args:
            html (str): the html content
        """
        self.html = html
