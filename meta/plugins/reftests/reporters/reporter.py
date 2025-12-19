from abc import ABC, abstractmethod
from cutekit import model
from pathlib import Path

from ..Test import TestCase


class Reporter(ABC):
    """
    Object to ensure every reporter has the same interface (and to clean the types).
    """

    def __init__(self, source_dir: Path, test_report: Path):
        pass

    @abstractmethod
    def addTestCase(self, test: TestCase, passed: bool, ):
        pass

    @abstractmethod
    def addTestCategory(self, props, file: Path, results):
        pass

    @abstractmethod
    def addSkippedFile(self, props):
        pass

    @abstractmethod
    def addSkippedCase(self, test: TestCase):
        pass

    @abstractmethod
    def finish(self, manifests: model.Registry, results, context):
        pass
