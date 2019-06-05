-- Speeds up the game
function QuickBots()
    Bot.MaxSpeed = 9
    Bot.MaxForce = 30
    Bot.SwapSpeed = -1

    Items.RespawnTime = -1
end

QuickBots()
