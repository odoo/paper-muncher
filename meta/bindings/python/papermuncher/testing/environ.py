import logging
import urllib

from papermuncher import Environ
from papermuncher.bindings import FAKE_REPORT_URI

logger = logging.getLogger(__name__)

class BaseTestEnviron(Environ):
    def get_asset(self, path: str) -> bytes:
        """Fetches a file from local filesystem if path starts with file:///,
        otherwise returns the HTML content.

        Args:
            path (str): the URI to get the file

        Returns:
            bytes: the file content, or fallback from parent class
        """
        if path in FAKE_REPORT_URI:
            return self.html.encode()


class TestEnvironMocked(BaseTestEnviron):
    __slots__ = ('html', 'data_dir')

    html: str
    data_dir: dict[str, str]

    def __init__(self, html: str, data_dir: dict[str, str]):
        super().__init__(html)
        self.data_dir = data_dir

    def get_asset(self, path: str) -> bytes:
        """Fetches mocked data from the data_dir if path is in it,
        otherwise returns the HTML content.

        Args:
            path (str): the URI to get the file

        Returns:
            bytes: the file content, or fallback from parent class
        """

        if data := self.data_dir.get(path):
            return data.encode()
        return super().get_asset(path)
