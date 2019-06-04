-- Quick 4 punch combo
function QuadLauncher()
    Bot.LauncherReload = 5
    Bot.LauncherDamage = 30
    Bot.LauncherSpread = math.pi/12
    Bot.LauncherRate = 1/120 --Should always fire
    Bot.LauncherDefaultAmmo = 4
    Bot.RocketSpeed = 15

    LauncherAmmo.Value = 8
end

QuadLauncher()
