// ============================================================================
// PASTA: 4_World/SDN_Entities
// ARQUIVO: SDN_ItemBase.c
// CAMADA: 4_World
// EXECUÇÃO: Server
// DESCRIÇÃO:
// Controle de apodrecimento de comida (Edible_Base).
// Lógica de roupas removida (está no PlayerBase).
// ============================================================================

modded class Edible_Base
{
    // Intercepta o processamento de decomposição (Rotten)
    override void ProcessDecay(float delta, bool hasRootAsPlayer)
    {
        if (GetGame().IsServer())
        {
            SDN_SeasonManager manager = SDN_SeasonManager.GetInstance();
            if (manager)
            {
                float decayMult = manager.GetFoodDecayMultiplier();

                // Aplica o multiplicador do JSON
                // Se decayMult > 1.0 (Verão), estraga mais rápido.
                // Se decayMult < 1.0 (Inverno), preserva.
                delta = delta * decayMult;

                // MELHORIA DE SOBREVIVÊNCIA: COMIDA CONGELADA
                // Se estivermos em um inverno severo (onde a comida custa a apodrecer)
                // e a opção estiver ativada, derrubamos drasticamente a temperatura da comida,
                // forçando o jogador a usar fogueiras para descongelar antes de comer,
                // caso contrário ele perderá "Heat Comfort" e ficará doente ao ingerir.
                if (manager.IsFrozenFoodEnabled() && decayMult <= 0.2)
                {
                    // Força a temperatura do alimento para o negativo (-15 a -5) gradativamente
                    float currentTemp = GetTemperature();
                    if (currentTemp > -10.0)
                    {
                        // Reduz um pouco por frame até congelar
                        SetTemperature(currentTemp - (1.5 * delta));
                    }
                }
            }
        }

        super.ProcessDecay(delta, hasRootAsPlayer);
    }
}