Name:       org.tizen.share-panel
Summary:    Share Panel
Version:    0.1.0
Release:    0
Group:      Applications/Core Applications
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
BuildRequires: pkgconfig(libtzplatform-config)

%description
Description: Share Panel

%prep
%setup -q

%build
%define _pkg_dir %{TZ_SYS_RO_APP}/%{name}
%define _pkg_shared_dir %{_pkg_dir}/shared
%define _pkg_data_dir %{_pkg_dir}/data
%define _sys_icons_dir %{_pkg_shared_dir}/res
%define _sys_packages_dir %{TZ_SYS_RO_PACKAGES}
%define _sys_license_dir %{TZ_SYS_SHARE}/license

export CFLAGS="${CFLAGS} -Wall"
export LDFLAGS="${LDFLAGS} -Wl,--hash-style=both -Wl,--as-needed -fpie"
cd CMake
cmake . -DINSTALL_PREFIX=%{_pkg_dir} \
	-DSYS_ICONS_DIR=%{_sys_icons_dir} \
	-DSYS_PACKAGES_DIR=%{_sys_packages_dir}
make %{?jobs:-j%jobs}
cd -

%install
cd CMake
%make_install
cd -

%define tizen_sign 1
%define tizen_sign_base %{_pkg_dir}
%define tizen_sign_level public
%define tizen_author_sign 1
%define tizen_dist_sign 1
%find_lang share-panel

%files -f share-panel.lang
%manifest %{name}.manifest
%{_pkg_dir}/bin/share-panel
%{_pkg_dir}/res/edje/*.edj
%{_pkg_dir}/res/images/*.png
%{_sys_packages_dir}/%{name}.xml
%{_sys_icons_dir}/share-panel.png


