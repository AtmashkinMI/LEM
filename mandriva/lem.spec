%define name    lem
%define version 0.11.11
%define release %mkrel 1

Name:           %{name}
Summary:        Linux Erasing Machine
Version:        %{version}
Release:        %{release}
Source0:        %{name}-%{version}.tar.gz
URL:            https://github.com/AtmashkinMI/LEM

Group:          File tools
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot
License:        GPL
BuildRequires:  libqt4-devel >= 4.7.0
Requires:       kernel-desktop >= 3.0.0
Requires:       gksu
Suggests:       nautilus-actions

%description
Securely erases sensitive information in Linux.

%prep
%setup -q -n %{name}-%{version}

%build
qmake
make

%install
rm -rf $RPM_BUILD_ROOT
make install INSTALL_ROOT=%{buildroot}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0755,root,root)
/usr/bin/LEM
/usr/share/LEM/doc/*
/usr/share/LEM/gtk/*
/usr/share/LEM/icons/*
/usr/share/LEM/profiles/*
/usr/share/LEM/translations/*
/usr/share/applications/LEM.desktop
/usr/share/pixmaps/LEM.svg
