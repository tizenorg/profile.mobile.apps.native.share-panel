%define Exec share-panel
%define AppInstallPath /usr/apps/%{name}

Name:       org.tizen.share-panel
Summary:    Share Panel
Version:    0.1.0
Release:    0
Group:      Application
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools

BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(capi-content-mime-type)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(feedback)
BuildRequires: pkgconfig(isf)
BuildRequires: pkgconfig(notification)

%description
Description: Share Panel

%prep
%setup -q

%package devel
Summary:    Share panel library (devel)
Group:      Application
Requires:   %{name} = %{version}-%{release}

%description devel
Development files needed to build software that needs Share panel.

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%cmake . -DCMAKE_INSTALL_PREFIX="%{AppInstallPath}" -DCMAKE_TARGET="%{Exec}" -DCMAKE_PACKAGE="%{name}"
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post

%files
%manifest %{name}.manifest
%{AppInstallPath}/bin/share-panel
%{AppInstallPath}/res/*
%{_datarootdir}/packages/%{name}.xml
%{AppInstallPath}/res/locale/*/LC_MESSAGES/*


