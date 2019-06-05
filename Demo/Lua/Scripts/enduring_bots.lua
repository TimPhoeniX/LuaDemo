-- Longer battle for more mayhem
function EnduringBots()
    Bot.DefaultHealth = 1000
    Bot.DefaultArmor = 2000
    
    Items.RespawnTime = 120
    HealthPack.Value = 500
    ArmorPack.Value = 500
end

EnduringBots()
