from dataclasses import dataclass

from ._internal import with_pmoptions_init
from .default import REPORT_URI, OUTPUT_URI

@with_pmoptions_init
@dataclass
class PMOptions:
    """
    Class to handle Paper Muncher options and arguments.
    This class is used to manage the options passed to Paper Muncher
    and provides a way to convert them into command line arguments.
    """

    auto: bool = True
    unsecure: bool = False
    sandbox: bool = False
    pipe: bool = False

    @property
    def is_piped:
        """Check if the output is piped."""
        return self.auto or self.sandbox

    @property
    def args(self):
        self_dict = self.__dict__
        args = []
        for key, value in self_dict.items():
            if key in ["auto", "mode", "out"]:
                continue
            if isinstance(value, bool):
                if value:
                    args.append(f"--{key}")
            elif isinstance(value, str):
                args.append(f"--{key}={value}")
            elif isinstance(value, Iterable):
                for item in value:
                    args.append(f"--{key}={item}")
