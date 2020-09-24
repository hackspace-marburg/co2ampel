# SPDX-FileCopyrightText: 2003-2020 Eelco Dolstra and the Nixpkgs/NixOS contributors
# SPDX-FileCopyrightText: 2020 Claudio Bley
# SPDX-FileCopyrightText: 2020 Alvar Penning
#
# SPDX-License-Identifier: MIT

# This file is part of the nixpkgs, https://github.com/NixOS/nixpkgs/pull/98381

{ mkDerivation, stdenv, fetchgit, fetchpatch, fetchFromGitHub, qmake, pkgconfig
, qtbase, qtsvg, qttools, qtserialport, boost, libgit2
}:

let
  # tag corresponding to a release, has no further relation
  fritzingTag = "CD-498";
  # SHA256 of the fritzing-parts HEAD on the develop  branch,
  # which contains the latest stable parts definitions
  partsSha = "023d0e492328b15a33a9b32e9aa871bcfdbfd010";
in

mkDerivation rec {
  pname = "fritzing";
  version = "0.9.4-${fritzingTag}";

  src = fetchFromGitHub {
    owner = "fritzing";
    repo = "fritzing-app";
    rev = fritzingTag;
    sha256 = "0aljj2wbmm1vd64nhj6lh9qy856pd5avlgydsznya2vylyz20p34";
  };

  parts = fetchgit {
    url = "https://github.com/fritzing/fritzing-parts.git";
    branchName = "develop";
    fetchSubmodules = false;
    name = "fritzing-parts";
    rev = partsSha;
    sha256 = "0n7nyx4bjbhz6r965fzny8h87cilvygsg51lfa2pd4pm80szcqps";
  };

  patches = [(fetchpatch {
    name = "fix-libgit2-version.patch";
    url = "https://github.com/fritzing/fritzing-app/commit/472951243d70eeb40a53b1f7e16e6eab0588d079.patch";
    sha256 = "0v1zi609cjnqac80xgnk23n54z08g1lia37hbzfl8jcq9sn9adak";
  })];

  buildInputs = [ qtbase qtsvg qtserialport boost libgit2 ];

  nativeBuildInputs = [ qmake pkgconfig qttools ];

  postPatch = ''
    substituteInPlace phoenix.pro \
      --replace 'LIBGIT_STATIC = true' 'LIBGIT_STATIC = false'

    substituteInPlace tools/linux_release_script/release.sh \
      --replace 'git status' 'echo >/dev/null' \
      --replace 'git clean' 'echo >/dev/null' \
      --replace 'git clone' 'echo >/dev/null' \
      --replace 'release_folder="' 'release_folder="$out" #' \
      --replace './Fritzing -db' '# run after fixup'

    substituteInPlace src/fapplication.cpp \
      --replace 'PartsChecker::getSha(dir.absolutePath());' '"${partsSha}";'
  '';

  buildPhase = ''
    bash tools/linux_release_script/release.sh ${version}
  '';

  installPhase = ''
    rm "$out/Fritzing" # remove script file
    mkdir "$out/bin"
    mv "$out/lib/Fritzing" "$out/bin/Fritzing"
    mkdir --parents "$out/share/applications" "$out/share/metainfo"
    mv --target-directory="$out/share/applications" "$out/org.fritzing.Fritzing.desktop"
    mv --target-directory="$out/share/metainfo" "$out/org.fritzing.Fritzing.appdata.xml"
    cp --recursive --no-target-directory "$parts" "$out/fritzing-parts"
  '';

  postFixup = ''
    # generate the parts.db file
    QT_QPA_PLATFORM=offscreen "$out/bin/Fritzing" -db "$out/fritzing-parts/parts.db" -pp "$out/fritzing-parts" -folder "$out"
  '';

  qmakeFlags = [ "phoenix.pro" ];

  meta = {
    description = "An open source prototyping tool for Arduino-based projects";
    homepage = "http://fritzing.org/";
    license = stdenv.lib.licenses.gpl3;
    maintainers = [ stdenv.lib.maintainers.robberer ];
    platforms = stdenv.lib.platforms.linux;
  };
}
