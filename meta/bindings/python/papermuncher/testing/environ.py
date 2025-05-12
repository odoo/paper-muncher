import logging
import urllib

from papermuncher import Environ
from papermuncher.bindings import FAKE_REPORT_URL

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
        if path in FAKE_REPORT_URL:
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


class TestEnvironWithRealPaths(BaseTestEnviron):
    def get_asset(self, path: str) -> bytes:
        """Fetches a file from local filesystem if path starts with file:///,
        otherwise downloads from the web.

        Danger:
            This method is NOT safe – use with caution!

        Args:
            path (str): the URI to get the file

        Returns:
            bytes: the file content, or fallback from parent class
        """
        if path.startswith('file:///'):
            local_path = path[len('file:///'):]
            try:
                with open(local_path, 'rb') as f:
                    return f.read()
            except FileNotFoundError:
                logger.error("Local file not found: %s", local_path)
            except PermissionError:
                logger.error("Permission denied when accessing: %s", local_path)
            except OSError as e:
                logger.error("OS error while accessing %s: %s", local_path, e)

        elif path.startswith(('http://', 'https://')):
            try:
                with urllib.request.urlopen(path, timeout=TIMEOUT) as response:
                    return response.read()
            except urllib.error.HTTPError as e:
                logger.error("HTTP error fetching %s: %s", path, e)
            except urllib.error.URLError as e:
                logger.error("URL error fetching %s: %s", path, e)
            except ValueError:
                logger.error("Invalid URL: %s", path)
            except Exception as e:
                logger.error("Unexpected error fetching %s: %s", path, e)

        return super().get_asset(path)
