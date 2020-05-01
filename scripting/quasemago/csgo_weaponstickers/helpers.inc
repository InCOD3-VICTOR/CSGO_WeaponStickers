stock void FindGameConfOffset(Handle gameConf, int &offset, char[] key)
{
	if ((offset = GameConfGetOffset(gameConf, key)) == -1)
	{
		SetFailState("Failed to get \"%s\" offset.", key);
	}
}

stock int FindSendPropOffset(char[] cls, char[] prop)
{
	int offset;
	if ((offset = FindSendPropInfo(cls, prop)) < 1)
	{
		SetFailState("Failed to find prop: \"%s\" on \"%s\" class.", prop, cls);
	}
	return offset;
}

stock bool AddMenuItemEx(Menu menu, const char[] info, int style = ITEMDRAW_DEFAULT, const char[] format, any ...)
{
	char buffer[254];
	VFormat(buffer, sizeof(buffer), format, 5);
	return menu.AddItem(info, buffer, style);
}

stock bool IsValidWeapon(int entity)
{
	if (entity < 0 || !IsValidEntity(entity))
	{
		return false;
	}

	char classname[64];
	if (!GetEntityClassname(entity, classname, sizeof(classname)))
	{
		return false;
	}

	return (strncmp(classname, "weapon_", 7, false) == 0);
}

stock bool ClientWeaponHasStickers(int client, int index)
{
	for (int i = 0; i < MAX_STICKERS_SLOT; i++)
	{
		if (g_PlayerWeapon[client][index].m_stickerIndex[i] != 0)
		{
			return true;
		}
	}
	return false;
}

stock int GetWeaponDefIndex(int weapon)
{
	return GetEntProp(weapon, Prop_Send, "m_iItemDefinitionIndex");
}

stock int GetWeaponIndexByEntity(int entity)
{
	if (!IsValidWeapon(entity))
	{
		return -1;
	}

	int defIndex = GetWeaponDefIndex(entity);
	if (defIndex < 0)
	{
		return -1;
	}

	char strDefIndex[MAX_LENGTH_INDEX];
	IntToString(defIndex, strDefIndex, sizeof(strDefIndex));

	int index;
	if (g_WeaponDefIndex.GetValue(strDefIndex, index))
	{
		return index;
	}

	return -1;
}

stock int GetWeaponIndexByClassname(const char[] classname)
{
	int index;
	if (g_WeaponClassName.GetValue(classname, index))
	{
		return index;
	}
	return -1;
}

stock bool GetWeaponClassnameByDefIndex(int defIndex, char[] buffer, int size)
{
	char strDefIndex[MAX_LENGTH_INDEX];
	IntToString(defIndex, strDefIndex, sizeof(strDefIndex));

	int index;
	if (g_WeaponDefIndex.GetValue(strDefIndex, index))
	{
		strcopy(buffer, size, g_Weapons[index].m_className);
		return true;
	}
	return false;
}

stock int GetWeaponReserveAmmo(int weapon)
{
	return GetEntProp(weapon, Prop_Send, "m_iPrimaryReserveAmmoCount");
}

stock void SetWeaponReserveAmmo(int weapon, int value)
{
	if (value >= 0)
	{
		SetEntProp(weapon, Prop_Send, "m_iPrimaryReserveAmmoCount", value);
	}
}

stock int GetWeaponClipAmmo(int weapon)
{
	return GetEntProp(weapon, Prop_Send, "m_iClip1");
}

stock void SetWeaponClipAmmo(int weapon, int value)
{
	if (value >= 0)
	{
		SetEntProp(weapon, Prop_Send, "m_iClip1", value);
	}
}

stock int GetStickerIndexByDefIndex(int defIndex, int &stickerSet)
{
	for (int i = 0; i < g_stickerSetsCount; i++)
	{
		for (int j = 0; j < g_stickerCount[i]; j++)
		{
			if (g_Sticker[i][j].m_defIndex == defIndex)
			{
				stickerSet = i;
				return j;
			}
		}
	}
	return -1;
}

stock bool GetStickerNameByDefIndex(int defIndex, char[] buffer, int size)
{
	for (int i = 0; i < g_stickerSetsCount; i++)
	{
		for (int j = 0; j < g_stickerCount[i]; j++)
		{
			if (g_Sticker[i][j].m_defIndex == defIndex)
			{
				strcopy(buffer, size, g_Sticker[i][j].m_displayName);
				return true;
			}
		}
	}
	return false;
}

stock bool GetStickerNameByDefIndexInSet(int defIndex, int stickerSet, char[] buffer, int size)
{
	for (int i = 0; i < g_stickerCount[stickerSet]; i++)
	{
		if (g_Sticker[stickerSet][i].m_defIndex == defIndex)
		{
			strcopy(buffer, size, g_Sticker[stickerSet][i].m_displayName);
			return true;
		}
	}
	return false;
}

stock int GetStickerSetByStickerIndex(int index)
{
	for (int i = 0; i < g_stickerSetsCount; i++)
	{
		for (int j = 0; j < g_stickerCount[i]; j++)
		{
			if (j == index)
			{
				return i;
			}
		}
	}
	return -1;
}

stock int GetWeaponSupportedStickerSlots(int weapon)
{
	Address pWeapon = GetEntityAddress(weapon);
	if (pWeapon == Address_Null)
	{
		return -1;
	}

	Address pItemDefition = SDKCall(g_SDKGetItemDefinition, pWeapon + view_as<Address>(g_EconItemOffset));
	if (pItemDefition == Address_Null)
	{
		return -1;
	}

	return SDKCall(g_SDKGetNumSupportedStickerSlots, pItemDefition);
}

stock bool IsValidWeaponToChange(int client, int defIndex, bool message = false, bool avoidKnife = false)
{
	if (defIndex <= 0)
	{
		return false;
	}
	else if (defIndex == 42 || defIndex == 59)
	{
		if (message)
		{
			CPrintToChat(client, "You can't change default knifes!");
		}
		return false;
	}
	else if (defIndex == 49)
	{
		if (message)
		{
			CPrintToChat(client, "You can't change c4!");
		}
		return false;
	}
	else if (defIndex >= 43 && defIndex <= 48)
	{
		if (message)
		{
			CPrintToChat(client, "You can't change grenades!");
		}
		return false;
	}

	char classname[64];
	if (!GetWeaponClassnameByDefIndex(defIndex, classname, sizeof(classname)))
	{
		return false;
	}

	if (IsKnifeClassname(classname) && avoidKnife)
	{
		return false;
	}
	return true;
}

stock bool IsKnifeClassname(const char[] classname)
{
	if ((StrContains(classname, "knife") > -1 && strcmp(classname, "weapon_knifegg") != 0) || StrContains(classname, "bayonet") > -1)
	{
		return true;
	}
	return false;
}

stock bool SetAttributeValue(int client, Address pEconItemView, int attrValue, const char[] format, any ...)
{
	char attr[254];
	VFormat(attr, sizeof(attr), format, 5);

	Address pAttributeDef = SDKCall(g_SDKGetAttributeDefinitionByName, g_pItemSchema, attr);
	if (pAttributeDef == Address_Null)
	{
		PrintToChat(client, "[SM] Invalid item attribute definition, contact an administrator.");
		return false;
	}

	// Get item attribute list.
	Address pAttributeList = pEconItemView + view_as<Address>(g_EconItemView_AttributeListOffset);

	// Get attributes data.
	int attrDefIndex = LoadFromAddress(pAttributeDef + view_as<Address>(g_EconItemAttributeDefinition_iAttributeDefinitionIndexOffset), NumberType_Int16);
	int attrCount = LoadFromAddress(pAttributeList + view_as<Address>(g_AttributeList_CountOffset), NumberType_Int32);

	// Checks if the item already has the attribute, then update value.
	int k = 0;
	for (int i = 0; i < attrCount; i++)
	{
		Address pAttribute = DereferencePointer(pAttributeList + view_as<Address>(g_AttributeList_ReadOffset)) + view_as<Address>(k);

		int defIndex = LoadFromAddress(pAttribute + view_as<Address>(g_Attributes_iAttributeDefinitionIndexOffset), NumberType_Int16);
		if (defIndex == attrDefIndex)
		{
			// Checks if the value is different.
			int value = LoadFromAddress(pAttribute + view_as<Address>(g_Attributes_iRawValue32Offset), NumberType_Int32);
			if (value != attrValue)
			{
				StoreToAddress(pAttribute + view_as<Address>(g_Attributes_iRawValue32Offset), attrValue, NumberType_Int32);
				return true;
			}
			return false;
		}

		// Increment index.
		k += 24;
	}

	// Didn't find it. Add a new one.
	if (g_ServerPlatform == OS_Windows)
	{
		MemoryBlock block = new MemoryBlock(24);
		Address pAttribute = block.Address;
		StoreToAddress(pAttribute + view_as<Address>(g_Attributes_iAttributeDefinitionIndexOffset), attrDefIndex, NumberType_Int16);
		StoreToAddress(pAttribute + view_as<Address>(g_Attributes_iRawValue32Offset), attrValue, NumberType_Int32);
		StoreToAddress(pAttribute + view_as<Address>(g_Attributes_iRawInitialValue32Offset), attrValue, NumberType_Int32);
		StoreToAddress(pAttribute + view_as<Address>(g_Attributes_nRefundableCurrencyOffset), 0, NumberType_Int32);
		StoreToAddress(pAttribute + view_as<Address>(g_Attributes_bSetBonusOffset), 0, NumberType_Int8);

		if (IsValidAddress(pAttribute))
		{
			SDKCall(g_SDKAddAttribute, pAttributeList, DereferencePointer(pEconItemView + view_as<Address>(172)), pAttribute);
			delete block; // Needed?
			return true;
		}
	}
	else
	{
		Address pAttribute = SDKCall(g_SDKGenerateAttribute, g_pItemSystem, attrDefIndex, view_as<float>(attrValue));
		if (IsValidAddress(pAttribute))
		{
			SDKCall(g_SDKAddAttribute, pAttributeList, pAttribute);
			return true;
		}
	}
	return false;
}

stock Address DereferencePointer(Address addr)
{
	return view_as<Address>(LoadFromAddress(addr, NumberType_Int32));
}

// Snipped from TF2 Attributes.
stock bool IsValidAddress(Address pAddress)
{
	static Address Address_MinimumValid = view_as<Address>(0x10000);
	if (pAddress == Address_Null)
	{
		return false;
	}
	return unsigned_compare(view_as<int>(pAddress), view_as<int>(Address_MinimumValid)) >= 0;
}

stock int unsigned_compare(int a, int b)
{
	if (a == b)
		return 0;
	if ((a >>> 31) == (b >>> 31))
		return ((a & 0x7FFFFFFF) > (b & 0x7FFFFFFF)) ? 1 : -1;
	return ((a >>> 31) > (b >>> 31)) ? 1 : -1;
}