import sys

from skbuild import setup

# Require pytest-runner only when running tests
pytest_runner = (['pytest-runner>=2.0,<3dev']
                 if any(arg in sys.argv for arg in ('pytest', 'test'))
                 else [])

setup_requires = pytest_runner

setup(
    name="hello",
    version="1.2.4",
    description="a minimal example package (cython version)",
    author='The scikit-build team',
    license="MIT",
    packages=['src'],
    package_dir={'hello': 'src/'},
    install_requires=['cython'],
    tests_require=['pytest'],
    setup_requires=setup_requires
)
