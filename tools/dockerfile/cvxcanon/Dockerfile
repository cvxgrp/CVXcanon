FROM debian:latest

# Debian packages
RUN apt-get update && apt-get install -y \
  autoconf \
  autotools-dev \
  build-essential \
  bzip2 \
  cmake \
  curl \
  g++ \
  gfortran \
  git \
  libc-dev \
  libopenblas-dev \
  libquadmath0 \
  libtool \
  make \
  parallel \
  pkg-config \
  python-decorator \
  python-dev \
  python-pil \
  python-setuptools \
  unzip \
  timelimit \
  wget \
  zip && apt-get clean

# Python with numpy/scipy and cvxpy
RUN easy_install -U pip
RUN pip install -U numpy scipy
RUN pip install -U cvxpy
RUN pip install -U nose wheel

# Get C++ linter
RUN wget https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py -O /usr/local/bin/cpplint.py
RUN chmod +x /usr/local/bin/cpplint.py

CMD ["bash"]
