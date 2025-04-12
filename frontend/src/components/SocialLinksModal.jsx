import { motion } from 'framer-motion';
import { FaGithub, FaLinkedin, FaInfoCircle } from 'react-icons/fa';

const SocialLinksModal = ({ onAboutClick }) => {
  return (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      className="fixed bottom-8 right-8 z-50"
    >
      <div className="glass p-4 rounded-xl shadow-xl">
        <div className="flex flex-col space-y-4">
          <button
            onClick={onAboutClick}
            className="text-gray-400 hover:text-accent transition-colors"
          >
            <FaInfoCircle size={24} />
          </button>
          <a
            href="https://github.com/Nischaya008"
            className="text-gray-400 hover:text-accent transition-colors"
            target="_blank"
            rel="noopener noreferrer"
          >
            <FaGithub size={24} />
          </a>
          <a
            href="https://www.linkedin.com/in/nischaya008/"
            className="text-gray-400 hover:text-accent transition-colors"
            target="_blank"
            rel="noopener noreferrer"
          >
            <FaLinkedin size={24} />
          </a>
        </div>
      </div>
    </motion.div>
  );
};

export default SocialLinksModal; 