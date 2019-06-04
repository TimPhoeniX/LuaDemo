-- Longer battle for more mayhem
function EnduringBots()
    Bot.DefaultHealth = 1000
    Bot.DefaulthArmor = 2000
    
    Item.RespawnTime = 120
    HealthPack.Value = 500
    ArmorPack.Value = 500
end

EnduringBots()
