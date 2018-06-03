pragma Singleton

import QtQuick 2.0

Item
{
	id: fonts

	readonly property FontLoader fontAwesomeRegular: FontLoader
	{
		source: "./fa-regular-400.ttf"
	}

	readonly property FontLoader fontAwesomeSolid: FontLoader
	{
		source: "./fa-solid-900.ttf"
	}

	readonly property FontLoader fontAwesomeBrands: FontLoader
	{
		source: "./fa-brands-400.ttf"
	}

	readonly property string icons: fonts.fontAwesomeBrands.name
	readonly property string solidIcons: fonts.fontAwesomeSolid.name
	readonly property string brands: fonts.fontAwesomeBrands.name
}
