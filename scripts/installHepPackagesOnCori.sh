#!/bin/bash
# -----------------------
# Requirements:
# gcc, 7.3.0
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

#EIGEN3_INCLUDE_DIR="/global/homes/x/xju/.local/Cellar/eigen3/include/eigen3"
EIGEN3_INCLUDE_DIR="/usr/local/Cellar/eigen/3.3.4/include/eigen3"

BASE_DIR=$PWD
INSTALL_DIR="$BASE_DIR/Extra"


PYTHON_DIR="/global/project/projectdirs/atlas/xju/miniconda3/envs/py2/"
PYTHON_INCLUDE_DIR="${PYTHON_DIR}/include/python2.7"
ROOT_DIR="/global/homes/x/xju/project//xju/software/root/v6-04-16_hsg7"


function download()
{
	fileName=`basename $1`
	if [ -e $fileName ];then
		echo $fileName is there
		return
	fi
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

function mk_dir()
{
 	DIR_NAME=$1	
	if [ ! -d $DIR_NAME ];then
		mkdir -pv ${DIR_NAME}
	fi

	cd $DIR_NAME
}

function hepmc() 
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="2.06.09"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "Version-->$VERSION"

	mk_dir HepMC

	HEPMC_FILE_NAME="HepMC-${VERSION}"

	if [ -e ${HEPMC_FILE_NAME} ]; then
		echo "${HEPMC_FILE_NAME} is there"
	else
		local URL="http://lcgapp.cern.ch/project/simu/HepMC/download/${HEPMC_FILE_NAME}.tar.gz"
		download $URL
	fi


	# install
	if [ ! -d ${HEPMC_FILE_NAME} ];then	
		tar xzf "${HEPMC_FILE_NAME}.tar.gz";
	fi

	cd ${HEPMC_FILE_NAME}
	INSTALL="${INSTALL_DIR}/${HEPMC_FILE_NAME}"
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]
	then
		./configure --prefix=${INSTALL} --with-momentum=GEV --with-length=MM
		make
		make install
	fi

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

	mk_dir "EvtGen"

	EVTGEN_FILE_NAME="EvtGen-${VERSION}"

	if [ -e ${EVTGEN_FILE_NAME} ];then
		echo $EVTGEN_FILE_NAME is there
	else
		local URL="http://www.hepforge.org/archive/evtgen/${EVTGEN_FILE_NAME}.tar.gz"
		download ${URL}
 	fi

	# install
	evt_gen_dir="EvtGen/R01-07-00/"
	if [ ! -d $evt_gen_dir ];then
		tar xzf ${EVTGEN_FILE_NAME}.tar.gz;
 	fi
	cd $evt_gen_dir

	INSTALL="${INSTALL_DIR}/${EVTGEN_FILE_NAME}"
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]; then
		./configure --hepmcdir=${INSTALL_DIR}/${HEPMC_FILE_NAME}\
			--prefix=${INSTALL}
		make
		make install
    fi

	cd ${BASE_DIR}
}

function fastjet()
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="3.3.0"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"

	mk_dir FastJet

	FASTJET_FILE_NAME="fastjet-${VERSION}"
	if [ -e ${FASTJET_FILE_NAME} ]; then
		echo ${FASTJET_FILE_NAME} is there
	else
		local URL="http://www.fastjet.fr/repo/${FASTJET_FILE_NAME}.tar.gz"
		download $URL
	fi

	# install
	if [ ! -d ${FASTJET_FILE_NAME} ];then
		tar xzf ${FASTJET_FILE_NAME}.tar.gz;
	fi
	cd  $FASTJET_FILE_NAME
	INSTALL=${INSTALL_DIR}/${FASTJET_FILE_NAME}
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]; then
		./configure --prefix=${INSTALL} --enable-allcxxplugins --enable-trackjet
		make
		make install
	fi

	cd ${BASE_DIR}
}

function lhapdf() 
{
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="6.2.1"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	mk_dir "LHAPDF"

	LHAPDF_FILE_NAME="LHAPDF-${VERSION}"

	if [ -e ${LHAPDF_FILE_NAME} ];then
		echo ${LHAPDF_FILE_NAME} is there
	else
		local URL="http://www.hepforge.org/archive/lhapdf/${LHAPDF_FILE_NAME}.tar.gz"
		download ${URL}
	fi

	# install
	if [ ! -d ${LHAPDF_FILE_NAME} ];then
		tar xzf ${LHAPDF_FILE_NAME}.tar.gz;
	fi
	cd  ${LHAPDF_FILE_NAME}

	INSTALL="${INSTALL_DIR}/${LHAPDF_FILE_NAME}"
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]; then
		./configure --prefix=${INSTALL} --disable-python
		make
		make install
	fi

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

	mk_dir "Pythia"

	PYTHIA_FILE_NAME="pythia${VERSION}"
	if [ -e ${PYTHIA_FILE_NAME} ]; then
		echo ${PYTHIA_FILE_NAME} is there
	else
		local URL="http://home.thep.lu.se/~torbjorn/pythia8/${PYTHIA_FILE_NAME}.tgz"
		download ${URL}
	fi

	# install
	if [ ! -d ${PYTHIA_FILE_NAME} ];then
		tar xzf ${PYTHIA_FILE_NAME}.tgz; 
	fi
	cd ${PYTHIA_FILE_NAME}

	INSTALL=${INSTALL_DIR}/${PYTHIA_FILE_NAME}
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]; then
		./configure --prefix=${INSTALL} \
			--with-evtgen=${INSTALL_DIR}/${EVTGEN_FILE_NAME} \
			--with-fastjet3=${INSTALL_DIR}/${FASTJET_FILE_NAME} \
			--with-hepmc2=${INSTALL_DIR}/${HEPMC_FILE_NAME} \
			--with-lhapdf6=${INSTALL_DIR}/${LHAPDF_FILE_NAME} \
			--with-root=${ROOT_DIR} \
			--with-python=${PYTHON_DIR} \
			--with-python-include=${PYTHON_INCLUDE_DIR} \
			--with-gzip-include=/usr/include \
			--with-gzip-lib=/usr/lib64 \
			--with-boost=/usr/common/software/boost/1.67.0/intel/haswell
		make
		make install
	fi

	cd ${BASE_DIR}
	echo "PYTHIA is installed"
}

function yoda(){
	local VERSION=$1
	if [ -z $VERSION ]; then
		VERSION="1.7.0"
		echo "Version is not given, ${VERSION} will be used"
	fi
	echo "version-->$VERSION"
	mk_dir "YODA"

	YODA_FILE_NAME="YODA-${VERSION}"

	if [ -e ${YODA_FILE_NAME} ];then
		echo ${YODA_FILE_NAME} is there
	else
		local URL="http://www.hepforge.org/archive/yoda/${YODA_FILE_NAME}.tar.gz"
		download ${URL}
	fi

	# install
	if [ ! -d ${YODA_FILE_NAME} ];then
		tar xzf ${YODA_FILE_NAME}.tar.gz;
	fi
	cd  ${YODA_FILE_NAME}

	INSTALL="${INSTALL_DIR}/${YODA_FILE_NAME}"
	if [[ $2 == "update" ]] || [ ! -e ${INSTALL} ]; then
		./configure --prefix=${INSTALL}
		make
		make install
	fi

	cd ${BASE_DIR}
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
	mk_dir "rivet"

	RIVET_FILE_NAME="Rivet-${VERSION}"
	if [ ! -e ${RIVET_FILE_NAME} ];then
		local URL="http://rivet.hepforge.org/hg/bootstrap/raw-file/${VERSION}/rivet-bootstrap-lcg"
		download ${URL}
	fi

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

	mk_dir "Professor"

	PROF_FILE_NAME="Professor-${VERSION}"
	if [ ! -e ${PROF_FILE_NAME} ];then
		local URL="http://www.hepforge.org/archive/professor/${PROF_FILE_NAME}.tar.gz"
		download ${URL}
	fi

	# install
	if [ ! -d  ${PROF_FILE_NAME} ]; then
		tar xzf ${PROF_FILE_NAME}.tar.gz; 
	fi
	cd  ${PROF_FILE_NAME}

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
		VERSION="2.6.3.2"
	fi
	echo "Downloading MadGraph --> ${VERSION}"

	MADGRAPH_FILE_NAME="MG5_aMC${VERSION}"
	mk_dir "MG5_aMC-${VERSION}"

	local FILENAME="MG5_aMC_v${VERSION}.tar.gz"
	if [ -e ${FILENAME} ];then
		echo ${FILENAME} is there
	else
		local URL="https://launchpad.net/mg5amcnlo/2.0/2.6.x/+download/${FILENAME}"
		download ${URL}
	fi

	# install
	DIR_MAD=${VERSION//\./_}
	if [ ! -d $DIR_MAD ];then
		tar xzf "${FILENAME}"
	fi	
	cd ${DIR_MAD}


	cd ${BASE_DIR}
}

# install packages one by one...
doup=""
hepmc
evtgen
fastjet 3.3.0 "update"
lhapdf
pythia 8235 update
#yoda
#rivet
#professor
#madgraph
