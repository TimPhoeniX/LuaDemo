function PrintRailgunParams()
    print("Railgun Parameters:")
    print(string.format("-Damage       : %f", Bot.RailgunReload))
    print(string.format("-Default Ammo : %u", Bot.RailgunDefaultAmmo))
    print(string.format("-Reload Time  : %f s", Bot.RailgunReload))
    print(string.format("-Firing Period: %f s", Bot.RailgunRate))
    print(string.format("-Shoot Spread : %f rad", Bot.RailgunSpread))
end

function PrintLauncherParams()
    print("Launcher Parameters:")
    print(string.format("-Damage       : %f", Bot.LauncherReload))
    print(string.format("-Default Ammo : %u", Bot.LauncherDefaultAmmo))
    print(string.format("-Reload Time  : %f s", Bot.LauncherReload))
    print(string.format("-Firing Period: %f s", Bot.LauncherRate))
    print(string.format("-Shoot Spread : %f rad", Bot.LauncherSpread))
    print(string.format("-Rocket Speed : %f m/s", Bot.RocketSpeed))
end

function PrintWeaponParams()
    PrintRailgunParams()
    PrintLauncherParams()
end

function PrintBotParams()
    print("Bot Parameters:")
    print(string.format("-Default Health     : %f", Bot.DefaultHealth))
    print(string.format("-Default Armor      : %f", Bot.DefaultArmor))
    print(string.format("-Max Speed          : %f m/s", Bot.MaxSpeed))
    print(string.format("-Max Steering Force : %f ", Bot.MaxForce))
    print(string.format("-Weapon Swap Time   : %f s", Bot.SwapSpeed))
end

function PrintBotProperties(bot)
    print(string.format("-Health/Armor: %f/%f",bot.Health, bot.Armor))
    print(string.format("-Currently %s", BotStateName[bot.State]))
    print(string.format("-Current Weapon: %s", WeaponName[bot.CurrentWeapon]))
    print(string.format("-Ammo RG, RL: %u/%u, %u/%u", bot.RGLoadedAmmo, bot.RGSpareAmmo, bot.RLLoadedAmmo, bot.RLSpareAmmo))
    if bot.IsSwappingWeapon then print("Bot is swapping weapon.") end
    if bot.IsSwappingWeapon then
        print("-Bot is reloading weapon.")
        print("-Remaining RG Reload Time: %f s", bot.RGReloadTime)
        print("-Remaining RL Reload Time: %f s", bot.RLReloadTime)
    end

end

function TableInvert(t)
    local inverted = {}
    for key, value in pairs(t) do
        inverted[value] = key
    end
    return inverted
end

BotStateName = TableInvert(BotState)
WeaponName = TableInvert(CurrentWeapon)
ItemName = TableInvert(ItemType)

function PrintAllBots()
    for index, bot in ipairs(Bots) do
        print("Bot " .. index)
        PrintBotProperties(bot)
    end
end

function ForAllBots(fun, val)
    for index, bot in ipairs(Bots) do
        fun(bot, val)
    end
end