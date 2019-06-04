-- Not really a shotgun
function Shotgun()
    Bot.RailgunReload = 5
    Bot.RailgunDamage = 30
    Bot.RailgunSpread = math.pi/8
    Bot.RailgunRate = 1/120 --Should always fire
    Bot.RailgunDefaultAmmo = 5

    RailgunAmmo.Value = 20
end

Shotgun()
