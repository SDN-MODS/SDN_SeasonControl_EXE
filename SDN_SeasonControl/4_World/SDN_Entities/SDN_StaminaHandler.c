// ============================================================================
// PASTA: 4_World/SDN_Entities
// ARQUIVO: SDN_StaminaHandler.c
// CAMADA: 4_World
// EXECUÇÃO: Server & Client
// DESCRIÇÃO:
// Modifica a recuperação de Stamina baseada na estação (Cansaço no verão).
// ============================================================================

modded class StaminaHandler
{
    override void Update(float deltaT, int pCurrentCommandID)
    {
        // Se a proteção estiver desativada ou Manager off, roda normal
        SDN_SeasonManager manager = SDN_SeasonManager.GetInstance();
        if (!manager || !manager.IsStaminaModifierEnabled())
        {
            super.Update(deltaT, pCurrentCommandID);
            return;
        }

        // O Vanilla deve ser executado primeiro para definir os deltas e predições base
        super.Update(deltaT, pCurrentCommandID);

        // ABORDAGEM SÊNIOR: Ao invés de tentar reduzir a taxa de recuperação
        // e brigar com o código Vanilla (o que causa Jitter e dessincronização no Client),
        // nós limitamos o MÁXIMO da Stamina do jogador (Cap) de acordo com o clima.
        // Em dias normais, ele tem 100% de fôlego (StaminaCap normal).
        // No calor extremo de Verão (staminaMult 0.7), o teto de fôlego cai 30%.

        float staminaMult = manager.GetStaminaRecoveryMultiplier();
        if (staminaMult < 1.0)
        {
            // Pega o CAP que o Vanilla calculou (baseado no peso/equipamento atual)
            // e corta usando a penalidade do clima.
            m_StaminaCap = m_StaminaCap * staminaMult;

            // Se o limite novo for menor que a stamina atual, esvazia
            // suavemente o excesso ou restringe o teto instantaneamente
            if (m_Stamina > m_StaminaCap)
            {
                m_Stamina = m_StaminaCap;
            }
        }
    }
}