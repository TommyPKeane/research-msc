# `wfmi` Python Package

`wfmi` is a custom Python 3.11 package for the __"Weighted and Filtered Mutual Information"__ algorithm, provided here only for instructive/research purposes, specifically to accompany the written M.Sc. Thesis provided in this repository.

⚠️ Note, as the other README files indicate, the commercial rights and Intellectual Property of this algorithm are retained by [Lenel Systems, Inc. (UTC/Carrier)](https://www.lenels2.com/en/us/) since 2010. If you're interested in this algorithm, you'll have to talk to them.

## Development Setup

The designed and tested configuration environment, here is on macOS with:

- [`direnv`](https://direnv.net/)
- [`pyenv`](https://github.com/pyenv/pyenv)

Both can be easily installed with [`brew`](https://brew.sh/), by first installing `brew` and then running the following commands:

1. `brew install direnv`
1. `brew install pyenv`

Once you have these files installed, you should then be able to run the following commands which will setup the local project directory to allow you to do encapsulated testing, here, without any risk to polluting your global system Python environment(s):

1. `pyenv install` -- Only needed if you don't have the version from the `.python-version` file already installed
1. `direnv allow` -- Create a local project directory and setup a locally encapsulated version of Python
1. `pip install --upgrade pip` -- Make sure the local `pip` is up-to-date for security and compatibility concerns
1. `pip install poetry` -- Locally install `poetry` for Package and Dependency Management
1. `poetry install` -- Install the Dependencies from the `poetry.lock` file, per the `pyproject.toml` Configuration

At this point you should be able to work in the Python interpreter directly, run the unit-tests through `pytest`, or follow the instructions below to spin-up Jupyter Lab for working interactively in a Jupyter/iPython Notebook.

If you're having any trouble with the above, you can review my preferred `bash` configuration for macOS in this repository: https://github.com/TommyPKeane/example-bash-configuration

And if you want to get some salty warnings about not relying too much on Jupyter/iPython Notebooks, you can look at this repository: https://github.com/TommyPKeane/jupyter_notebooks

## Testing

### Unit-Testing

Unit-Testing is provided by custom `pytest` implementations in the `tests_unit/` directory, configured _via_ the `pyproject.toml` and `.coveragerc` files.

You can run the current unit-tests with the following command:

```bash
pytest
```

If you want to modify any of the configuration options, you'll need to refer to the [`pytest` Documentation](https://docs.pytest.org/en/7.2.x/reference/customize.html).


### Jupyter Notebook

If you want an interactive iPython/Jupyter Notebook to have a MATLAB-like interface to be able to use the `wfmi` package, you'll need to navigate to the `tests_notebooks/` directory and start-up `jupyterlab` using the following commands:

1. `cd tests_notebooks/`
1. `direnv allow`
1. `pip install --upgrade pip`
1. `pip install poetry`
1. `poetry install`
1. `jupyterlab`

The `jupyterlab` command should start a local webserver and provide a link that you can open in your preferred Web Browser, if it doesn't open automatically. You should then have access to navigate within the `tests_notebooks/` directory and open (or create) any `.ipynb` notebook JSON files to work interactively.

This process and the directory structure with associated configuration files has been designed in a way to locally install the `wfmi` package into the scope of the `tests_notebooks/` directory through the `tests_notebooks/pyproject.toml` file, _via_ `poetry`.

There are other ways to do this, but this is all prepared to be easily accessible with only `direnv` and `pyenv` as the prerequisites, which avoid both the need for running Docker or polluting your global system Python configuration.
