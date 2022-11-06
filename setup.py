#!/usr/bin/env python3

import os
import shutil
import subprocess

from setuptools import setup
from setuptools.command.install import install


class CustomInstall(install):
    """Custom handler for the 'install' command."""

    def run(self):
        target = "intel-power-control-helper"
        targetdir = self.install_scripts

        subprocess.check_call(f"cc -Wall {target}.c -o {target}", shell=True)
        os.makedirs(targetdir, exist_ok=True)
        shutil.copy(target, targetdir)
        os.chmod(os.path.join(targetdir, target), 0o755)

        super().run()


with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="intel-power-control",
    author="Joerg Mechnich",
    author_email="joerg.mechnich@gmail.com",
    license="GNU GPLv3",
    description="GPU power management for Intel hardware on Linux",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/jmechnich/intel-power-control",
    use_scm_version={"local_scheme": "no-local-version"},
    setup_requires=["setuptools_scm"],
    install_requires=["PyQt5"],
    scripts=["intel-power-control", "intel-power-control-setup"],
    data_files=[
        ("share/applications", ["misc/intel-power-control.desktop"]),
        ("share/icons/hicolor/32x32/apps", ["misc/intel-power-control.png"]),
        ("share/icons/hicolor/scalable/apps", ["misc/intel-power-control.svg"]),
    ],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: POSIX :: Linux",
        "Environment :: X11 Applications :: Qt",
    ],
    python_requires=">=3.6",
    cmdclass={"install": CustomInstall},
)
