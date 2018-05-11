#!/bin/bash
# -----------------------
# Requirements:
# gcc, 6.2.0
# eigen3, 3.2.5
# python, 2.7.13
	# pip
	# numpy, matplotlib, cypython
# -----------------------

# -----------------------
# Installs following packages:
# Pythia, 8.2.35
	# EvtGen, 01.07.00
	# FastJet, 3.3.0
	# HepMC, 2.06.09 (frozen, no development)
	# LHAPDF, 6.2.1
# rivet, 2.6.0
# Professor, 2.2.2
# -----------------------

which python
which gcc

EIGEN3_INCLUDE_DIR="/global/homes/x/xju/.local/Cellar/eigen3/include/eigen3"
CYTHON_DIR="/global/homes/x/xju/.local/Cellar/python/2.7.13/bin/cython"

BASE_DIR=$PWD
INSTALL_DIR="$BASE_DIR/Extra"


ROOT_DIR="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/root/6.10.04-x86_64-slc6-gcc62-opt"
PYTHON_DIR="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/python/2.7.13-x86_64-slc6-gcc62/2.7.13-597a5/x86_64-slc6-gcc62-opt"
PYTHON_INCLUDE_DIR="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/x86_64/python/2.7.13-x86_64-slc6-gcc62/2.7.13-597a5/x86_64-slc6-gcc62-opt/include/python2.7"


function download()
{
	# later add an option for mac.
	wget $1 --no-check-certificate
}

IsInstalled="True"
function check()
{
	if [ -e ${INSTALL_DIR}/$1 ]; then
		IsInstalled="True"
	else
		IsInstalled="False"
	fi
}

function hepmc() 
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="2.06.09"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="HepMC"
	HEPMC_FILE_NAME="hepmc${VERSION}"
	local URL="http://hepmc.web.cern.ch/hepmc/releases/${HEPMC_FILE_NAME}.tgz"

	check ${HEPMC_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${HEPMC_FILE_NAME} already installed
		return;
	fi

	echo "Installing HepMC"
	cd ${BASE_DIR}
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf "${HEPMC_FILE_NAME}.tgz"; cd ${HEPMC_FILE_NAME}
	./configure --prefix=${INSTALL_DIR}/$HEPMC_FILE_NAME --with-momentum=GEV --with-length=mm
	make
	make install

	cd ${BASE_DIR}
}

function evtgen()
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="01.07.00"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="EvtGen"
	EVTGEN_FILE_NAME="EvtGen-${VERSION}"
	local URL="http://www.hepforge.org/archive/evtgen/${EVTGEN_FILE_NAME}.tar.gz"
	echo "Installing EvtGen"

	check ${EVTGEN_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${EVTGEN_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf ${EVTGEN_FILE_NAME}.tar.gz; cd EvtGen/R01-07-00/

	./configure --hepmcdir=${INSTALL_DIR}/${HEPMC_FILE_NAME} --prefix=${INSTALL_DIR}/${EVTGEN_FILE_NAME}
	make
	make install

	cd ${BASE_DIR}
	echo "EvtGen is installed"
}

function fastjet()
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="3.3.0"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="FastJet"
	FASTJET_FILE_NAME="fastjet-${VERSION}"
	local URL="http://www.fastjet.fr/repo/${FASTJET_FILE_NAME}.tar.gz"
	echo "Installing FastJet"

	check ${FASTJET_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${FASTJET_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf ${FASTJET_FILE_NAME}.tar.gz; cd  $FASTJET_FILE_NAME

	#./configure --prefix=${INSTALL_DIR}/${FASTJET_FILE_NAME} --enable-trackjet --enable-atlascone
	./configure --prefix=${INSTALL_DIR}/${FASTJET_FILE_NAME} --enable-allcxxplugins --enable-trackjet
	make
	make install

	cd ${BASE_DIR}
	echo "FastJet is installed"
}

function lhapdf() 
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="6.2.1"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="LHAPDF"
	LHAPDF_FILE_NAME="LHAPDF-${VERSION}"
	local URL="http://www.hepforge.org/archive/lhapdf/${LHAPDF_FILE_NAME}.tar.gz"
	echo "Installing LHAPDF"

	check ${LHAPDF_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${LHAPDF_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf ${LHAPDF_FILE_NAME}.tar.gz; cd  ${LHAPDF_FILE_NAME}

	./configure --prefix=${INSTALL_DIR}/${LHAPDF_FILE_NAME}
	make
	make install

	cd ${BASE_DIR}
	echo "LHAPDF is installed"
}

function pythia()
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="8235"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="Pythia"
	PYTHIA_FILE_NAME="pythia${VERSION}"
	local URL="http://home.thep.lu.se/~torbjorn/pythia8/${PYTHIA_FILE_NAME}.tgz"
	echo "Installing PYTHIA"

	check ${PYTHIA_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${PYTHIA_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf ${PYTHIA_FILE_NAME}.tgz; cd ${PYTHIA_FILE_NAME}

	./configure --prefix=${INSTALL_DIR}/${PYTHIA_FILE_NAME} \
		--with-evtgen=${INSTALL_DIR}/${EVTGEN_FILE_NAME} \
		--with-fastjet3=${INSTALL_DIR}/${FASTJET_FILE_NAME} \
		--with-hepmc2=${INSTALL_DIR}/${HEPMC_FILE_NAME} \
		--with-lhapdf6=${INSTALL_DIR}/${LHAPDF_FILE_NAME} \
		--with-root=${ROOT_DIR} \
		--with-python=${PYTHON_DIR} \
		--with-python-include=${PYTHON_INCLUDE_DIR}

	make
	make install

	cd ${BASE_DIR}
	echo "PYTHIA is installed"
}

function rivet()
{
	# https://rivet.hepforge.org/trac/wiki/GettingStarted
	# based on LCG!
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="2.6.0"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	RIVET_FILE_NAME="Rivet-${VERSION}"
	local DIR_NAME="rivet"
	local URL="http://rivet.hepforge.org/hg/bootstrap/raw-file/${VERSION}/rivet-bootstrap-lcg"
	echo "Installing rivet"

	check ${RIVET_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${RIVET_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	chmod +x rivet-bootstrap-lcg
	INSTALL_PREFIX=${INSTALL_DIR}/${RIVET_FILE_NAME} MAKE="make -j8"  RIVET_VERSION=${VERSION} ./rivet-bootstrap-lcg

	cd ${BASE_DIR}
	echo "rivet is installed"
}

function professor()
{
	# https://professor.hepforge.org/trac/wiki#Installation
	# need to setup Rivet first!
	VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="2.2.2"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	local DIR_NAME="Professor"
	PROF_FILE_NAME="Professor-${VERSION}"
	local URL="http://www.hepforge.org/archive/professor/${PROF_FILE_NAME}.tar.gz"
	echo "Installing PROF"

	check ${PROF_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${PROF_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	tar xzf ${PROF_FILE_NAME}.tar.gz; cd  ${PROF_FILE_NAME}

	make CPPFLAGS="-I${EIGEN3_INCLUDE_DIR}" CYTHON=${CYTHON_DIR}
	make install PREFIX=${INSTALL_DIR}/${PROF_FILE_NAME}
	cd ${BASE_DIR}
	echo "PROF is installed"

}

# MadGraph5_aMC@NLO 2.6.2
function madgraph() 
{
	VERSION=$1
	if [ -z $VERSION ];then
		VERSION="2.6.2"
	fi
	echo "Downloading MadGraph --> ${VERSION}"

	MADGRAPH_FILE_NAME="MG5_aMC${VERSION}"
	local DIR_NAME="MG5_aMC-${VERSION}"
	local FILENAME="MG5_aMC_v${VERSION}.tar.gz"
	local URL="https://launchpad.net/mg5amcnlo/2.0/2.6.x/+download/${FILENAME}"

	check ${MADGRAPH_FILE_NAME}
	if [ "${IsInstalled}" = "True" ]; then
		echo ${MADGRAPH_FILE_NAME} already installed
		return;
	fi

	cd $BASE_DIR
	mkdir -pv $DIR_NAME; cd $DIR_NAME; download $URL

	# install
	echo "Installing"
	tar xzf "${FILENAME}"; cd ${VERSION//\./_}



	cd ${BASE_DIR}
}

# install packages one by one...

hepmc
evtgen # need to check the directory
fastjet
lhapdf
pythia
rivet
professor

