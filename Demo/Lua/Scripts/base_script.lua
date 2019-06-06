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
    if bot.IsSwappingWeapon then print("-Bot is swapping weapon.") end
    if bot.IsSwappingWeapon then
        print("-Bot is reloading weapon.")
        print("-Remaining RG Reload Time: %f s", bot.RGReloadTime)
        print("-Remaining RL Reload Time: %f s", bot.RLReloadTime)
    end

end

function PrintItemParams()
    print(string.format("-Item respawn time  : %f", Items.RespawnTime))
    print(string.format("-Health Pack Value  : %f", HealthPack.Value))
    print(string.format("-Armor Pack Value   : %f", ArmorPack.Value))
    print(string.format("-Railgun Ammo Value : %u", RailgunAmmo.Value))
    print(string.format("-Launcher Ammo Value: %u", LauncherAmmo.Value))
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

function ForAllBots(fun, ...)
    for index, bot in ipairs(Bots) do
        fun(bot, unpack(arg))
    end
end

function ResetRailgun()
    Bot.RailgunReload = 10
    Bot.RailgunDamage = 100
    Bot.RailgunSpread = 0.05
    Bot.RailgunRate = 1.5
    Bot.RailgunDefaultAmmo = 5

    RailgunAmmo.Value = 5
end

function ResetLauncher()
    Bot.LauncherReload = 5
    Bot.LauncherDamage = 165
    Bot.LauncherSpread = 0.05
    Bot.LauncherRate = 1
    Bot.LauncherDefaultAmmo = 15
    Bot.RocketSpeed = 5

    LauncherAmmo.Value = 15
end

function ResetBot()
    Bot.DefaultHealth = 100 
    Bot.DefaultArmor = 250
    Bot.MaxSpeed = 3
    Bot.MaxForce = 15
    Bot.SwapSpeed = 3

    Item.RespawnTime = 15
    HealthPack.Value = 50
    ArmorPack.Value = 50
end
