-- That's a lot of damage
function Fireworks()
    Bot.LauncherReload = 10
    Bot.LauncherDamage = 10
    Bot.LauncherSpread = math.pi/8
    Bot.LauncherRate = 1/120
    Bot.LauncherDefaultAmmo = 12
    Bot.RocketSpeed = 10

    LauncherAmmo.Value = 36
end

Fireworks()
