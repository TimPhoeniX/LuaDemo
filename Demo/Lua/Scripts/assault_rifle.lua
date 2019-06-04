-- Inaccurate, but sure to hit
function AssaultRifle()
    Bot.RailgunReload = 5
    Bot.RailgunDamage = 10
    Bot.RailgunSpread = math.pi/4
    Bot.RailgunRate = 1/10
    Bot.RailgunDefaultAmmo = 30

    RailgunAmmo.Value = 90
end

AssaultRifle()
